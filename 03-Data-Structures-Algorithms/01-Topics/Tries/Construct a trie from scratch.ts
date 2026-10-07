/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time Complexity: Insert O(L), Search O(L), StartsWith O(L)
// Space Complexity: O(ALPHABET_SIZE * L * N)

class TrieNode {
    children: Map<string, TrieNode>;
    isEndOfWord: boolean;

    constructor() {
        this.children = new Map();
        this.isEndOfWord = false;
    }
}

class Trie {
    root: TrieNode;

    constructor() {
        this.root = new TrieNode();
    }

    insert(key: string): void {
        let curr = this.root;
        for (const char of key) {
            if (!curr.children.has(char)) {
                curr.children.set(char, new TrieNode());
            }
            curr = curr.children.get(char)!;
        }
        curr.isEndOfWord = true;
    }

    search(key: string): boolean {
        let curr = this.root;
        for (const char of key) {
            if (!curr.children.has(char)) {
                return false;
            }
            curr = curr.children.get(char)!;
        }
        return curr.isEndOfWord;
    }

    startsWith(prefix: string): boolean {
        let curr = this.root;
        for (const char of prefix) {
            if (!curr.children.has(char)) {
                return false;
            }
            curr = curr.children.get(char)!;
        }
        return true;
    }
}

// Example driver
const keys = ["the", "a", "there", "answer", "any", "by", "bye", "their"];
const trie = new Trie();
for (const key of keys) {
    trie.insert(key);
}

console.log("the:", trie.search("the") ? "Present" : "Not present");
console.log("these:", trie.search("these") ? "Present" : "Not present");
console.log("th (prefix):", trie.startsWith("th") ? "Present" : "Not present");
