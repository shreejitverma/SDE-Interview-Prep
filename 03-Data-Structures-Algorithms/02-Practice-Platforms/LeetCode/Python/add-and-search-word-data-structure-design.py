# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
# Space: O(N * L) total heap memory across all nodes

class TrieNode:
    def __init__(self):
        self.children = {}
        self.is_end = False

class WordDictionary:
    def __init__(self):
        self.root = TrieNode()

    def addWord(self, word: str) -> None:
        curr = self.root
        for ch in word:
            if ch not in curr.children:
                curr.children[ch] = TrieNode()
            curr = curr.children[ch]
        curr.is_end = True

    def search(self, word: str) -> bool:
        def dfs(index: int, curr: TrieNode) -> bool:
            if index == len(word):
                return curr.is_end

            ch = word[index]
            if ch == '.':
                for child in curr.children.values():
                    if dfs(index + 1, child):
                        return True
                return False
            else:
                if ch not in curr.children:
                    return False
                return dfs(index + 1, curr.children[ch])

        return dfs(0, self.root)
