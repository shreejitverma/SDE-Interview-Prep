/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

class WordDictionary {
    private static class TrieNode {
        boolean isEnd = false;
        TrieNode[] children = new TrieNode[26];
    }

    private final TrieNode root;

    public WordDictionary() {
        root = new TrieNode();
    }

    public void addWord(String word) {
        TrieNode curr = root;
        for (int i = 0; i < word.length(); i++) {
            int idx = word.charAt(i) - 'a';
            if (curr.children[idx] == null) {
                curr.children[idx] = new TrieNode();
            }
            curr = curr.children[idx];
        }
        curr.isEnd = true;
    }

    public boolean search(String word) {
        return searchInNode(word, 0, root);
    }

    private boolean searchInNode(String word, int index, TrieNode curr) {
        if (curr == null) {
            return false;
        }
        if (index == word.length()) {
            return curr.isEnd;
        }

        char ch = word.charAt(index);
        if (ch == '.') {
            for (TrieNode child : curr.children) {
                if (child != null && searchInNode(word, index + 1, child)) {
                    return true;
                }
            }
            return false;
        } else {
            int idx = ch - 'a';
            return searchInNode(word, index + 1, curr.children[idx]);
        }
    }
}
