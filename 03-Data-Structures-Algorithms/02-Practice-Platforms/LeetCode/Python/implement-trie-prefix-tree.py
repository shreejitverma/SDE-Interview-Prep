# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma

# Time:  O(L) per insert / search / startsWith operation where L is string length
# Space: O(N * L) where N is number of words inserted


class TrieNode:
    def __init__(self) -> None:
        self.children: dict[str, TrieNode] = {}
        self.is_end: bool = False


class Trie:
    def __init__(self) -> None:
        self.root = TrieNode()

    def insert(self, word: str) -> None:
        curr = self.root
        for ch in word:
            if ch not in curr.children:
                curr.children[ch] = TrieNode()
            curr = curr.children[ch]
        curr.is_end = True

    def _find_prefix(self, prefix: str) -> TrieNode | None:
        curr = self.root
        for ch in prefix:
            if ch not in curr.children:
                return None
            curr = curr.children[ch]
        return curr

    def search(self, word: str) -> bool:
        node = self._find_prefix(word)
        return node is not None and node.is_end

    def startsWith(self, prefix: str) -> bool:
        return self._find_prefix(prefix) is not None
