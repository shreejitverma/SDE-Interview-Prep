/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: LRU Cache (Least Recently Used)
 * Description: O(1) get and put operations using std::list (Doubly Linked List) and std::unordered_map.
 */

#include <iostream>
#include <list>
#include <unordered_map>
#include <optional>

class LRUCache {
private:
    size_t capacity;
    
    // Doubly Linked List: Stores {key, value} pairs.
    // Most recently used items are at the front.
    std::list<std::pair<int, int>> lru_list;
    
    // Hash Map: Maps Key -> Iterator to the list node.
    // Allows O(1) access to any node in the list.
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache_map;

public:
    LRUCache(size_t cap) : capacity(cap) {}

    int get(int key) {
        // 1. Check if key exists
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return -1; // Not found
        }

        // 2. Move accessed item to front of list (Mark as recently used)
        // splice moves elements between lists (or within same list) in O(1)
        lru_list.splice(lru_list.begin(), lru_list, it->second);

        return it->second->second; // Return value
    }

    void put(int key, int value) {
        auto it = cache_map.find(key);

        if (it != cache_map.end()) {
            // Key exists: Update value and move to front
            it->second->second = value;
            lru_list.splice(lru_list.begin(), lru_list, it->second);
        } else {
            // Key does not exist
            if (cache_map.size() == capacity) {
                // Cache full: Remove LRU item (back of list)
                int lru_key = lru_list.back().first;
                lru_list.pop_back();
                cache_map.erase(lru_key);
            }
            
            // Insert new item at front
            lru_list.push_front({key, value});
            cache_map[key] = lru_list.begin();
        }
    }
};

int main() {
    LRUCache lru(2); // Capacity 2

    lru.put(1, 10);
    lru.put(2, 20);
    std::cout << "Get 1: " << lru.get(1) << "\n"; // Returns 10, Key 1 moves to front

    lru.put(3, 30); // Evicts Key 2 (LRU)
    std::cout << "Get 2: " << lru.get(2) << "\n"; // Returns -1 (Not found)

    lru.put(4, 40); // Evicts Key 1 (LRU)
    std::cout << "Get 1: " << lru.get(1) << "\n"; // Returns -1 (Not found)
    std::cout << "Get 3: " << lru.get(3) << "\n"; // Returns 30
    std::cout << "Get 4: " << lru.get(4) << "\n"; // Returns 40

    return 0;
}
