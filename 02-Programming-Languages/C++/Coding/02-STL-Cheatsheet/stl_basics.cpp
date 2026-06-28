/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: C++ STL Cheatsheet for Interviews
 * Description: Essential containers and algorithms with time complexities.
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <queue>

void vector_demo() {
    // std::vector (Dynamic Array)
    // Access: O(1), Search: O(N), Insert/Delete (End): O(1), Insert/Delete (Middle): O(N)
    std::vector<int> v = {1, 5, 2, 4, 3};
    v.push_back(6);
    std::sort(v.begin(), v.end()); // O(N log N)
    
    // Binary Search (Requires sorted array)
    bool exists = std::binary_search(v.begin(), v.end(), 4); // O(log N)
    
    // Lower Bound (First element >= value)
    auto it = std::lower_bound(v.begin(), v.end(), 3); // O(log N)
    std::cout << "Lower bound of 3: " << *it << "\n";
}

void map_demo() {
    // std::unordered_map (Hash Map)
    // Avg: O(1), Worst: O(N) - Use for most lookup tasks
    std::unordered_map<std::string, int> umap;
    umap["Alice"] = 100;
    if (umap.find("Alice") != umap.end()) {
        std::cout << "Found Alice\n";
    }

    // std::map (Balanced BST - Red Black Tree)
    // Always O(log N). Keys are sorted.
    std::map<int, std::string> ordered_map;
    ordered_map[10] = "Ten";
    ordered_map[5] = "Five";
    // Iterating prints: 5, 10 (Sorted order)
}

void priority_queue_demo() {
    // Max Heap by default
    // Push/Pop: O(log N), Top: O(1)
    std::priority_queue<int> max_heap; 
    
    // Min Heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;
    
    min_heap.push(10);
    min_heap.push(5);
    std::cout << "Min Heap Top: " << min_heap.top() << "\n"; // 5
}

int main() {
    vector_demo();
    map_demo();
    priority_queue_demo();
    return 0;
}
