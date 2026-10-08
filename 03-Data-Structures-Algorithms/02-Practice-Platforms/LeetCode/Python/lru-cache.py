"""
Problem: LeetCode 146 - LRU Cache
Difficulty: Medium
Concepts: Hash Table, Linked List, Design, Doubly-Linked List

Time Complexity: O(1) for both get and put
Space Complexity: O(capacity)
"""

from collections import OrderedDict


class LRUCache:
    def __init__(self, capacity: int):
        self.capacity = capacity
        self.cache: OrderedDict[int, int] = OrderedDict()

    def get(self, key: int) -> int:
        if key not in self.cache:
            return -1
        self.cache.move_to_end(key)
        return self.cache[key]

    def put(self, key: int, value: int) -> None:
        if key in self.cache:
            self.cache[key] = value
            self.cache.move_to_end(key)
            return

        if len(self.cache) >= self.capacity:
            self.cache.popitem(last=False)

        self.cache[key] = value
