/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: Insert O(L), Search O(L), StartsWith O(L)
// Space Complexity: O(ALPHABET_SIZE * L * N)

package main

import "fmt"

const AlphabetSize = 26

type TrieNode struct {
	children    [AlphabetSize]*TrieNode
	isEndOfWord bool
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
	curr.isEndOfWord = true
}

func (t *Trie) Search(word string) bool {
	curr := t.root
	for i := 0; i < len(word); i++ {
		idx := word[i] - 'a'
		if curr.children[idx] == nil {
			return false
		}
		curr = curr.children[idx]
	}
	return curr != nil && curr.isEndOfWord
}

func (t *Trie) StartsWith(prefix string) bool {
	curr := t.root
	for i := 0; i < len(prefix); i++ {
		idx := prefix[i] - 'a'
		if curr.children[idx] == nil {
			return false
		}
		curr = curr.children[idx]
	}
	return curr != nil
}

func main() {
	keys := []string{"the", "a", "there", "answer", "any", "by", "bye", "their"}
	trie := Constructor()

	for _, key := range keys {
		trie.Insert(key)
	}

	fmt.Printf("the: %v\n", trie.Search("the"))
	fmt.Printf("these: %v\n", trie.Search("these"))
	fmt.Printf("th (prefix): %v\n", trie.StartsWith("th"))
}
