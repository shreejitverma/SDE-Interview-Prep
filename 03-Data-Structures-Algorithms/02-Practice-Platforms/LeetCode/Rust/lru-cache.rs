use std::collections::HashMap;

/*
 * Problem: LeetCode 146 - LRU Cache
 * Difficulty: Medium
 * Concepts: Hash Table, Linked List, Design, Doubly-Linked List
 *
 * Time Complexity: O(1) for both get and put
 * Space Complexity: O(capacity)
 */

struct Node {
    key: i32,
    value: i32,
    prev: usize,
    next: usize,
}

pub struct LRUCache {
    capacity: usize,
    map: HashMap<i32, usize>,
    nodes: Vec<Node>,
    free: Vec<usize>,
    head: usize,
    tail: usize,
}

impl LRUCache {
    pub fn new(capacity: i32) -> Self {
        let cap = capacity as usize;
        // Head dummy at index 0, tail dummy at index 1
        let mut nodes = Vec::with_capacity(cap + 2);
        nodes.push(Node { key: 0, value: 0, prev: 0, next: 1 });
        nodes.push(Node { key: 0, value: 0, prev: 0, next: 1 });

        Self {
            capacity: cap,
            map: HashMap::with_capacity(cap),
            nodes,
            free: Vec::new(),
            head: 0,
            tail: 1,
        }
    }

    fn remove_node(&mut self, idx: usize) {
        let prev = self.nodes[idx].prev;
        let next = self.nodes[idx].next;
        self.nodes[prev].next = next;
        self.nodes[next].prev = prev;
    }

    fn add_to_head(&mut self, idx: usize) {
        let next = self.nodes[self.head].next;
        self.nodes[idx].prev = self.head;
        self.nodes[idx].next = next;
        self.nodes[self.head].next = idx;
        self.nodes[next].prev = idx;
    }

    fn move_to_head(&mut self, idx: usize) {
        self.remove_node(idx);
        self.add_to_head(idx);
    }

    pub fn get(&mut self, key: i32) -> i32 {
        if let Some(&idx) = self.map.get(&key) {
            self.move_to_head(idx);
            self.nodes[idx].value
        } else {
            -1
        }
    }

    pub fn put(&mut self, key: i32, value: i32) {
        if let Some(&idx) = self.map.get(&key) {
            self.nodes[idx].value = value;
            self.move_to_head(idx);
            return;
        }

        if self.map.len() >= self.capacity {
            let lru_idx = self.nodes[self.tail].prev;
            self.remove_node(lru_idx);
            let lru_key = self.nodes[lru_idx].key;
            self.map.remove(&lru_key);
            self.free.push(lru_idx);
        }

        let new_idx = if let Some(idx) = self.free.pop() {
            self.nodes[idx] = Node { key, value, prev: 0, next: 0 };
            idx
        } else {
            let idx = self.nodes.len();
            self.nodes.push(Node { key, value, prev: 0, next: 0 });
            idx
        };

        self.map.insert(key, new_idx);
        self.add_to_head(new_idx);
    }
}
