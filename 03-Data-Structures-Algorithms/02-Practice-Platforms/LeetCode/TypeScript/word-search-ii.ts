/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 212 - Word Search II
 * Difficulty: Hard
 * Language: TypeScript
 *
 * Performance Analysis:
 * - Time Complexity: O(M * N * 4 * 3^(L - 1)) search, pruned proactively upon finding words.
 * - Space Complexity: O(Sum(len(words))) for Trie storage, O(L) recursion stack.
 */

class TrieNode {
    children: Map<string, TrieNode> = new Map();
    word: string | null = null;
    wordCount: number = 0;
}

function buildTrie(words: string[]): TrieNode {
    const root = new TrieNode();
    for (const w of words) {
        let curr = root;
        for (const ch of w) {
            if (!curr.children.has(ch)) {
                curr.children.set(ch, new TrieNode());
            }
            curr = curr.children.get(ch)!;
            curr.wordCount++;
        }
        curr.word = w;
    }
    return root;
}

export function findWords(board: string[][], words: string[]): string[] {
    const result: string[] = [];
    if (!board || board.length === 0 || board[0].length === 0 || words.length === 0) {
        return result;
    }

    const root = buildTrie(words);
    const m = board.length;
    const n = board[0].length;

    function dfs(r: number, c: number, parent: TrieNode, key: string): void {
        const curr = parent.children.get(key);
        if (!curr || curr.wordCount <= 0) {
            return;
        }

        if (curr.word !== null) {
            result.push(curr.word);
            curr.word = null;

            // Prune word count
            let node: TrieNode | undefined = curr;
            node.wordCount--;
            if (node.wordCount <= 0) {
                parent.children.delete(key);
            }
        }

        const original = board[r][c];
        board[r][c] = '#'; // Mark visited

        const dr = [-1, 1, 0, 0];
        const dc = [0, 0, -1, 1];

        for (let i = 0; i < 4; ++i) {
            const nr = r + dr[i];
            const nc = c + dc[i];
            if (nr >= 0 && nr < m && nc >= 0 && nc < n) {
                const nextChar = board[nr][nc];
                if (nextChar !== '#' && curr.children.has(nextChar)) {
                    dfs(nr, nc, curr, nextChar);
                }
            }
        }

        board[r][c] = original; // Backtrack
    }

    for (let r = 0; r < m; ++r) {
        for (let c = 0; c < n; ++c) {
            const ch = board[r][c];
            if (root.children.has(ch)) {
                dfs(r, c, root, ch);
            }
        }
    }

    return result;
}

// Standalone execution test
if (typeof require !== 'undefined' && require.main === module) {
    const board = [
        ['o', 'a', 'a', 'n'],
        ['e', 't', 'a', 'e'],
        ['i', 'h', 'k', 'r'],
        ['i', 'f', 'l', 'v']
    ];
    const words = ['oath', 'pea', 'eat', 'rain'];
    console.log('Found words:', findWords(board, words));
}
