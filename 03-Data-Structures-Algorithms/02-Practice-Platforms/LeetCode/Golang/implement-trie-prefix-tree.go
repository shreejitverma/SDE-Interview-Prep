/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(L) per insert / search / startsWith operation where L is string length
// Space: O(N * L) where N is number of words inserted

package main

type TrieNode struct {
	children [26]*TrieNode
	isEnd    bool
}

type Trie struct {
	root *TrieNode
}

func Constructor() Trie {
	return Trie{root: &TrieNode{}}
}

func (t *Trie) Insert(word string) {
	curr := t.root
	for i := 0; i < len(word); i++ {
		idx := word[i] - 'a'
		if curr.children[idx] == nil {
			curr.children[idx] = &TrieNode{}
		}
		curr = curr.children[idx]
	}
	curr.isEnd = true
}

func (t *Trie) findPrefix(prefix string) *TrieNode {
	curr := t.root
	for i := 0; i < len(prefix); i++ {
		idx := prefix[i] - 'a'
		if curr.children[idx] == nil {
			return nil
		}
		curr = curr.children[idx]
	}
	return curr
}

func (t *Trie) Search(word string) bool {
	node := t.findPrefix(word)
	return node != nil && node.isEnd
}

func (t *Trie) StartsWith(prefix string) bool {
	return t.findPrefix(prefix) != nil
}
