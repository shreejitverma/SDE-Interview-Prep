---
title: LeetCode - 0146 - LRU Cache
tags:
  - leetcode
  - problem
  - hash-table
  - linked-list
  - design
  - doubly-linked-list
difficulty: medium
source: leetcode
problem_number: "0146"
topics:
  - Hash Table
  - Linked List
  - Design
  - Doubly-Linked List
---

# LeetCode 0146: LRU Cache

## Problem Breakdown

Design a data structure that follows the constraints of a Least Recently Used (LRU) cache.
Implement the `LRUCache` class:
- `LRUCache(int capacity)`: Initialize the LRU cache with positive size `capacity`.
- `int get(int key)`: Return the value of the `key` if the key exists, otherwise return `-1`.
- `void put(int key, int value)`: Update the value of the `key` if the `key` exists. Otherwise, add the `key-value` pair to the cache. If the number of keys exceeds the `capacity` from this operation, evict the least recently used key.
The functions `get` and `put` must each run in $O(1)$ average time complexity.

### Structural Requirements

1. **Fast Key Lookup ($O(1)$)**: A hash table is mandatory to locate elements by key in constant expected time.
2. **Fast Recency Ordering ($O(1)$)**: Moving an existing item to the front or evicting the oldest item from the tail requires constant-time pointer mutations.
3. **Synergy of Hash Map and Doubly-Linked List**:
   - The hash map maps each `key` to its corresponding node in a doubly-linked list.
   - The doubly-linked list maintains items ordered by access recency.
   - Using sentinel `dummy head` and `dummy tail` nodes eliminates edge cases associated with null pointers during insertion and deletion.

## Optimal Approaches

### Doubly-Linked List + Hash Map Architecture

```
Cache State (capacity = 2):
head <-> [node 2 (val 2)] <-> [node 1 (val 1)] <-> tail
MRU: node 2
LRU: node 1

Operation: put(3, 3)
1. Cache is full (size 2 == capacity 2).
2. Evict LRU: tail.prev is node 1.
3. Remove node 1 from list and map.
4. Insert node 3 at head.
New State:
head <-> [node 3 (val 3)] <-> [node 2 (val 2)] <-> tail
```

1. Initialize `dummy head` and `dummy tail` connected to each other (`head.next = tail`, `tail.prev = head`).
2. `get(key)`:
   - If `key` is absent in map, return `-1`.
   - Retrieve node from map.
   - Detach node from its current position and insert it immediately after `head`.
   - Return node value.
3. `put(key, value)`:
   - If `key` already exists, update its value and move node to head.
   - If `key` is new:
     - If map size equals `capacity`, identify `tail.prev`, unlink it, and delete its key from map.
     - Create new node, insert it after `head`, and record it in map.

## Complexity Analysis

| Metric | Complexity | Notes |
| :--- | :--- | :--- |
| **`get(key)` Time** | $O(1)$ | Hash table lookup followed by $O(1)$ pointer relinking. |
| **`put(key, val)` Time** | $O(1)$ | Hash table insert/update and at most one node eviction in $O(1)$. |
| **Space Complexity** | $O(\text{capacity})$ | Exactly one hash entry and one linked list node per cached item. |

## Common Traps & Edge Cases

- **Zero Allocation Relinking**: In C++, `std::list::splice` transfers existing nodes directly between positions without calling `new` or `delete`.
- **Sentinel Dummies**: Omitting dummy head and tail nodes leads to extensive null checks when the list is empty or has a single element.
- **Eviction Key Lookup**: The linked list node must store both `key` and `value` so that during eviction of `tail.prev`, the key can be looked up and erased from the hash map.
