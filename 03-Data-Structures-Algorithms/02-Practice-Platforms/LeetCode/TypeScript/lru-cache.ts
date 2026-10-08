/*
 * Problem: LeetCode 146 - LRU Cache
 * Difficulty: Medium
 * Concepts: Hash Table, Linked List, Design, Doubly-Linked List
 *
 * Time Complexity: O(1) for both get and put
 * Space Complexity: O(capacity)
 */

export class LRUCache {
    private capacity: number;
    private cache: Map<number, number>;

    constructor(capacity: number) {
        this.capacity = capacity;
        this.cache = new Map();
    }

    get(key: number): number {
        if (!this.cache.has(key)) {
            return -1;
        }

        const value = this.cache.get(key)!;
        // Refresh recency by re-inserting at the end of Map order
        this.cache.delete(key);
        this.cache.set(key, value);
        return value;
    }

    put(key: number, value: number): void {
        if (this.cache.has(key)) {
            this.cache.delete(key);
        } else if (this.cache.size >= this.capacity) {
            // Evict least recently used (first key in iteration order)
            const firstKey = this.cache.keys().next().value;
            if (firstKey !== undefined) {
                this.cache.delete(firstKey);
            }
        }

        this.cache.set(key, value);
    }
}
