/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 *
 * Problem: LeetCode 212 - Word Search II
 * Difficulty: Hard
 * Language: Java
 *
 * Performance Analysis:
 * - Time Complexity: O(M * N * 4 * 3^(L - 1)) worst-case search, where M x N is board dimension
 *   and L is maximum word length. Trie leaf pruning rapidly collapses search space.
 * - Space Complexity: O(Sum(len(words))) to store words in the prefix Trie. Recursion depth O(L).
 */

import java.util.ArrayList;
import java.util.List;

public class Solution {

    private static class TrieNode {
        TrieNode[] children = new TrieNode[26];
        String word = null;
        int wordCount = 0; // Tracks remaining words reachable through this subtree for proactive pruning
    }

    private void insert(TrieNode root, String word) {
        TrieNode curr = root;
        for (int i = 0; i < word.length(); ++i) {
            int idx = word.charAt(i) - 'a';
            if (curr.children[idx] == null) {
                curr.children[idx] = new TrieNode();
            }
            curr = curr.children[idx];
            curr.wordCount++;
        }
        curr.word = word;
    }

    public List<String> findWords(char[][] board, String[] words) {
        List<String> result = new ArrayList<>();
        if (board == null || board.length == 0 || board[0].length == 0 || words == null || words.length == 0) {
            return result;
        }

        TrieNode root = new TrieNode();
        for (String w : words) {
            insert(root, w);
        }

        int m = board.length;
        int n = board[0].length;

        for (int r = 0; r < m; ++r) {
            for (int c = 0; c < n; ++c) {
                int idx = board[r][c] - 'a';
                if (idx >= 0 && idx < 26 && root.children[idx] != null) {
                    dfs(board, r, c, root, result);
                }
            }
        }

        return result;
    }

    private void dfs(char[][] board, int r, int c, TrieNode parent, List<String> result) {
        char ch = board[r][c];
        int idx = ch - 'a';
        TrieNode curr = parent.children[idx];
        if (curr == null || curr.wordCount <= 0) {
            return;
        }

        if (curr.word != null) {
            result.add(curr.word);
            curr.word = null; // Prevent duplicate additions
            // Decrement wordCount upwards along the matched path
            pruneTrie(parent, idx);
        }

        board[r][c] = '#'; // Mark cell visited in-place

        int[] dr = {-1, 1, 0, 0};
        int[] dc = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr >= 0 && nr < board.length && nc >= 0 && nc < board[0].length) {
                char nextCh = board[nr][nc];
                if (nextCh != '#') {
                    int nextIdx = nextCh - 'a';
                    if (curr.children[nextIdx] != null && curr.children[nextIdx].wordCount > 0) {
                        dfs(board, nr, nc, curr, result);
                    }
                }
            }
        }

        board[r][c] = ch; // Backtrack cell
    }

    private void pruneTrie(TrieNode parent, int idx) {
        TrieNode child = parent.children[idx];
        child.wordCount--;
        if (child.wordCount <= 0) {
            parent.children[idx] = null; // Sever empty branch
        }
    }

    public static void main(String[] args) {
        Solution sol = new Solution();
        char[][] board = {
            {'o', 'a', 'a', 'n'},
            {'e', 't', 'a', 'e'},
            {'i', 'h', 'k', 'r'},
            {'i', 'f', 'l', 'v'}
        };
        String[] words = {"oath", "pea", "eat", "rain"};
        List<String> res = sol.findWords(board, words);
        System.out.println("Found words: " + res);
    }
}
