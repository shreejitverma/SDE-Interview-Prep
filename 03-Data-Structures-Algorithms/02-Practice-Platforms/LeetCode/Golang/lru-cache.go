package main

/*
 * Problem: LeetCode 146 - LRU Cache
 * Difficulty: Medium
 * Concepts: Hash Table, Linked List, Design, Doubly-Linked List
 *
 * Time Complexity: O(1) for both get and put
 * Space Complexity: O(capacity)
 */

type LRUNode struct {
	key   int
	value int
	prev  *LRUNode
	next  *LRUNode
}

type LRUCache struct {
	capacity int
	cache    map[int]*LRUNode
	head     *LRUNode
	tail     *LRUNode
}

func Constructor(capacity int) LRUCache {
	head := &LRUNode{}
	tail := &LRUNode{}
	head.next = tail
	tail.prev = head

	return LRUCache{
		capacity: capacity,
		cache:    make(map[int]*LRUNode),
		head:     head,
		tail:     tail,
	}
}

func (this *LRUCache) removeNode(node *LRUNode) {
	node.prev.next = node.next
	node.next.prev = node.prev
}

func (this *LRUCache) addToHead(node *LRUNode) {
	node.next = this.head.next
	node.prev = this.head
	this.head.next.prev = node
	this.head.next = node
}

func (this *LRUCache) moveToHead(node *LRUNode) {
	this.removeNode(node)
	this.addToHead(node)
}

func (this *LRUCache) Get(key int) int {
	if node, exists := this.cache[key]; exists {
		this.moveToHead(node)
		return node.value
	}
	return -1
}

func (this *LRUCache) Put(key int, value int) {
	if node, exists := this.cache[key]; exists {
		node.value = value
		this.moveToHead(node)
		return
	}

	if len(this.cache) >= this.capacity {
		lru := this.tail.prev
		this.removeNode(lru)
		delete(this.cache, lru.key)
	}

	newNode := &LRUNode{key: key, value: value}
	this.cache[key] = newNode
	this.addToHead(newNode)
}
