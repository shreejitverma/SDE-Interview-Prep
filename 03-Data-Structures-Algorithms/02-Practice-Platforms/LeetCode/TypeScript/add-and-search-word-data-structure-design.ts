/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  addWord: O(L), search: O(26^D * L) where D is dot count, L is length
// Space: O(N * L) total heap memory across all nodes

class TrieNode {
    isEnd: boolean = false;
    children: (TrieNode | null)[] = new Array(26).fill(null);
}

class WordDictionary {
    private root: TrieNode;

    constructor() {
        this.root = new TrieNode();
    }

    addWord(word: string): void {
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

    search(word: string): boolean {
        return this.dfs(word, 0, this.root);
    }

    private dfs(word: string, index: number, curr: TrieNode): boolean {
        if (index === word.length) {
            return curr.isEnd;
        }

        const ch = word[index];
        if (ch === '.') {
            for (let i = 0; i < 26; i++) {
                const child = curr.children[i];
                if (child && this.dfs(word, index + 1, child)) {
                    return true;
                }
            }
            return false;
        } else {
            const idx = ch.charCodeAt(0) - 97;
            const child = curr.children[idx];
            return child ? this.dfs(word, index + 1, child) : false;
        }
    }
}
