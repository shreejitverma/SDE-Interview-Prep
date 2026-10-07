/*
 * Problem: LeetCode 130 - Surrounded Regions
 * Difficulty: Medium
 * Concepts: Graph, Matrix, DFS, BFS
 *
 * Time Complexity: O(m * n)
 * Space Complexity: O(m * n) recursion stack
 */

export function solve(board: string[][]): void {
    if (!board || board.length === 0 || board[0].length === 0) {
        return;
    }

    const m = board.length;
    const n = board[0].length;

    function dfs(r: number, c: number): void {
        if (r < 0 || r >= m || c < 0 || c >= n || board[r][c] !== "O") {
            return;
        }

        board[r][c] = "#";
        dfs(r + 1, c);
        dfs(r - 1, c);
        dfs(r, c + 1);
        dfs(r, c - 1);
    }

    // Step 1: Mark boundary-connected 'O's
    for (let i = 0; i < m; i++) {
        dfs(i, 0);
        dfs(i, n - 1);
    }
    for (let j = 0; j < n; j++) {
        dfs(0, j);
        dfs(m - 1, j);
    }

    // Step 2: Flip unvisited 'O' -> 'X', and '#' -> 'O'
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (board[i][j] === "O") {
                board[i][j] = "X";
            } else if (board[i][j] === "#") {
                board[i][j] = "O";
            }
        }
    }
}
