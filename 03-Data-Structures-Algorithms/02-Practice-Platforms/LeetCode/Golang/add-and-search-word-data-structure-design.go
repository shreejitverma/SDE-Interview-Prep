/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

package main

type WordDictionary struct {
	isEnd    bool
	children [26]*WordDictionary
}

func Constructor() WordDictionary {
	return WordDictionary{}
}

func (this *WordDictionary) AddWord(word string) {
	curr := this
	for i := 0; i < len(word); i++ {
		idx := word[i] - 'a'
		if curr.children[idx] == nil {
			curr.children[idx] = &WordDictionary{}
		}
		curr = curr.children[idx]
	}
	curr.isEnd = true
}

func (this *WordDictionary) Search(word string) bool {
	return this.searchHelper(word, 0)
}

func (this *WordDictionary) searchHelper(word string, index int) bool {
	if index == len(word) {
		return this.isEnd
	}

	ch := word[index]
	if ch == '.' {
		for i := 0; i < 26; i++ {
			if this.children[i] != nil && this.children[i].searchHelper(word, index+1) {
				return true
			}
		}
		return false
	}

	idx := ch - 'a'
	if this.children[idx] == nil {
		return false
	}
	return this.children[idx].searchHelper(word, index+1)
}
