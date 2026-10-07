/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: Insert O(L), Search O(L), StartsWith O(L) where L is key length
// Space Complexity: O(ALPHABET_SIZE * L * N)

class Trie {
    private static final int ALPHABET_SIZE = 26;

    static class TrieNode {
        TrieNode[] children;
        boolean isEndOfWord;

        TrieNode() {
            children = new TrieNode[ALPHABET_SIZE];
            isEndOfWord = false;
        }
    }

    private final TrieNode root;

    public Trie() {
        root = new TrieNode();
    }

    public void insert(String key) {
        TrieNode curr = root;
        for (int i = 0; i < key.length(); i++) {
            int index = key.charAt(i) - 'a';
            if (curr.children[index] == null) {
                curr.children[index] = new TrieNode();
            }
            curr = curr.children[index];
        }
        curr.isEndOfWord = true;
    }

    public boolean search(String key) {
        TrieNode curr = root;
        for (int i = 0; i < key.length(); i++) {
            int index = key.charAt(i) - 'a';
            if (curr.children[index] == null) {
                return false;
            }
            curr = curr.children[index];
        }
        return curr != null && curr.isEndOfWord;
    }

    public boolean startsWith(String prefix) {
        TrieNode curr = root;
        for (int i = 0; i < prefix.length(); i++) {
            int index = prefix.charAt(i) - 'a';
            if (curr.children[index] == null) {
                return false;
            }
            curr = curr.children[index];
        }
        return curr != null;
    }

    public static void main(String[] args) {
        String[] keys = {"the", "a", "there", "answer", "any", "by", "bye", "their"};
        Trie trie = new Trie();

        for (String key : keys) {
            trie.insert(key);
        }

        System.out.println("the: " + (trie.search("the") ? "Present" : "Not present"));
        System.out.println("these: " + (trie.search("these") ? "Present" : "Not present"));
        System.out.println("th (prefix): " + (trie.startsWith("th") ? "Present" : "Not present"));
    }
}
