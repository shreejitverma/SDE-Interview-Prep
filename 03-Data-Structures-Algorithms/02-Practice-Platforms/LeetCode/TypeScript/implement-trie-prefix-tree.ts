/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(L) per insert / search / startsWith operation where L is string length
// Space: O(N * L) where N is number of words inserted

class TrieNode {
    children: (TrieNode | null)[];
    isEnd: boolean;

    constructor() {
        this.children = new Array(26).fill(null);
        this.isEnd = false;
    }
}

class Trie {
    private root: TrieNode;

    constructor() {
        this.root = new TrieNode();
    }

    insert(word: string): void {
        let curr = this.root;
        for (let i = 0; i < word.length; i++) {
            const idx = word.charCodeAt(i) - 97;
            if (!curr.children[idx]) {
                curr.children[idx] = new TrieNode();
            }
            curr = curr.children[idx]!;
        }
        curr.isEnd = true;
    }

    private findPrefix(prefix: string): TrieNode | null {
        let curr = this.root;
        for (let i = 0; i < prefix.length; i++) {
            const idx = prefix.charCodeAt(i) - 97;
            if (!curr.children[idx]) {
                return null;
            }
            curr = curr.children[idx]!;
        }
        return curr;
    }

    search(word: string): boolean {
        const node = this.findPrefix(word);
        return node !== null && node.isEnd;
    }

    startsWith(prefix: string): boolean {
        return this.findPrefix(prefix) !== null;
    }
}
