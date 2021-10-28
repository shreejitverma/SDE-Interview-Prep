'''https://leetcode.com/problems/word-search-ii/
212. Word Search II
Hard

4925

171

Add to List

Share
Given an m x n board of characters and a list of strings words, return all words on the board.

Each word must be constructed from letters of sequentially adjacent cells, where adjacent cells are horizontally or vertically neighboring. The same letter cell may not be used more than once in a word.

 

Example 1:


Input: board = [["o","a","a","n"],["e","t","a","e"],["i","h","k","r"],["i","f","l","v"]], words = ["oath","pea","eat","rain"]
Output: ["eat","oath"]
Example 2:


Input: board = [["a","b"],["c","d"]], words = ["abcb"]
Output: []
 

Constraints:

m == board.length
n == board[i].length
1 <= m, n <= 12
board[i][j] is a lowercase English letter.
1 <= words.length <= 3 * 104
1 <= words[i].length <= 10
words[i] consists of lowercase English letters.
All the strings of words are unique.'''


class Trie:

    def __init__(self):
        """
        Initialize your data structure here.
        """
        self.trie = {}

    def insert(self, word: str) -> None:
        """
        Inserts a word into the trie.
        """
        t = self.trie
        for i in word:
            if i not in t:
                t[i] = {}
            t = t[i]
        t['#'] = True

    def search(self, word: str) -> bool:
        """
        Returns if the word is in the trie.
        """
        t = self.trie
        for i in word:
            if i not in t:
                return False
            t = t[i]
        if '#' not in t:
            return False
        return True

    def startsWith(self, prefix: str) -> bool:
        """
        Returns if there is any word in the trie that starts with the given prefix.
        """
        t = self.trie
        for i in prefix:
            if i not in t:
                return False
            t = t[i]
        return True


class Solution:
    def findWords(self, board: List[List[str]], words: List[str]) -> List[str]:
        trieNode = Trie()
        for word in words:
            trieNode.insert(word)
        res = []

        h, w = len(board), len(board[0])

        def dfs(i, j, path, node):
            if '#' in node.keys() and node['#']:
                res.append(path)
                node['#'] = False
            if i < 0 or i >= h or j < 0 or j >= w:
                return
            tmp = board[i][j]
            node = node.get(tmp, None)
            if not node:
                return
            board[i][j] = "*"
            dfs(i+1, j, path+tmp, node)
            dfs(i-1, j, path+tmp, node)
            dfs(i, j-1, path+tmp, node)
            dfs(i, j+1, path+tmp, node)
            board[i][j] = tmp

        for i in range(h):
            for j in range(w):
                dfs(i, j, "", trieNode.trie)
        return res
