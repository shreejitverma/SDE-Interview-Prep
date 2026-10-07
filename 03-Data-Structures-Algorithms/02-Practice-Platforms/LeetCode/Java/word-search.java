/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(M * N * 4^L) worst case, heavily pruned in practice
// Space: O(L) recursion stack

class Solution {
    public boolean exist(char[][] board, String word) {
        int m = board.length;
        int n = board[0].length;
        int len = word.length();
        if (m * n < len) {
            return false;
        }

        int[] boardFreq = new int[128];
        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                boardFreq[board[r][c]]++;
            }
        }
        for (int i = 0; i < len; i++) {
            if (--boardFreq[word.charAt(i)] < 0) {
                return false;
            }
        }

        // Optimization: Reverse word if last char is rarer than first char on board
        String target = word;
        if (boardFreq[word.charAt(0)] > boardFreq[word.charAt(len - 1)]) {
            target = new StringBuilder(word).reverse().toString();
        }

        for (int r = 0; r < m; r++) {
            for (int c = 0; c < n; c++) {
                if (board[r][c] == target.charAt(0)) {
                    if (dfs(board, r, c, target, 0)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private boolean dfs(char[][] board, int r, int c, String word, int idx) {
        if (idx == word.length()) {
            return true;
        }
        if (r < 0 || r >= board.length || c < 0 || c >= board[0].length || board[r][c] != word.charAt(idx)) {
            return false;
        }

        char temp = board[r][c];
        board[r][c] = '#'; // Mark visited

        boolean found = dfs(board, r + 1, c, word, idx + 1)
                     || dfs(board, r - 1, c, word, idx + 1)
                     || dfs(board, r, c + 1, word, idx + 1)
                     || dfs(board, r, c - 1, word, idx + 1);

        board[r][c] = temp; // Backtrack
        return found;
    }
}
